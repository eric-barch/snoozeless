import { SSEStreamingApi } from "hono/streaming";
import { HTTPException } from "hono/http-exception";
import { Context } from "hono";
import { SupabaseClient } from "@supabase/supabase-js";

export const enrollDeviceService = async (c: Context) => {
  const supabaseClient = c.get("supabaseClient");

  const { data: sessionData, error: sessionError } =
    await supabaseClient.auth.getSession();

  if (sessionError) {
    throw new HTTPException(400, { message: sessionError.message });
  }

  const session = sessionData.session;

  if (!session) {
    throw new HTTPException(400, { message: "Session not found." });
  }

  const userId = session.user.id;

  const { data: insertData, error: insertError } = await supabaseClient
    .from("devices")
    .insert({ user_id: userId })
    .select()
    .single();

  if (insertError) {
    throw new HTTPException(400, { message: insertError.message });
  }

  return insertData;
};

export const getDeviceStateService = async (
  c: Context,
  stream: SSEStreamingApi,
  deviceId: string,
) => {
  const supabaseClient: SupabaseClient = c.get("supabaseClient");

  const deviceStateChannel = supabaseClient
    .channel("device-state")
    .on(
      "postgres_changes",
      {
        event: "UPDATE",
        schema: "public",
        table: "devices",
        filter: `id=eq.${deviceId}`,
      },
      async (deviceUpdate) => {
        await stream.writeSSE({
          event: "device-update",
          data: JSON.stringify(deviceUpdate.new),
        });
      },
    )
    .on(
      "postgres_changes",
      {
        event: "*",
        schema: "public",
        table: "alarms",
        filter: `device_id=eq.${deviceId}`,
      },
      async (alarmChange) => {
        let event;
        let data;

        switch (alarmChange.eventType) {
          case "INSERT":
            event = "alarm-insert";
            data = JSON.stringify(alarmChange.new);
            break;
          case "DELETE":
            event = "alarm-delete";
            data = JSON.stringify(alarmChange.old);
            break;
          case "UPDATE":
            event = "alarm-update";
            data = JSON.stringify(alarmChange.new);
            break;
        }

        await stream.writeSSE({
          event,
          data,
        });
      },
    )
    .subscribe(async (status, error) => {
      if (status === "SUBSCRIBED") {
        const { data: deviceData, error: deviceError } = await supabaseClient
          .from("devices")
          .select("*")
          .eq("id", deviceId)
          .single();

        if (deviceError) {
          await stream.writeSSE({
            event: "device-error",
            data: JSON.stringify(deviceError),
          });
        } else if (deviceData) {
          await stream.writeSSE({
            event: "initial-device",
            data: JSON.stringify(deviceData),
          });
        }

        const { data: alarmData, error: alarmError } = await supabaseClient
          .from("alarms")
          .select("*")
          .eq("device_id", deviceId);

        if (alarmError) {
          await stream.writeSSE({
            event: "alarm-error",
            data: JSON.stringify(alarmError),
          });
        } else if (alarmData) {
          await stream.writeSSE({
            event: "initial-alarms",
            data: JSON.stringify(alarmData),
          });
        }

        console.log("Subscribed to device state.");
      } else if (status === "CHANNEL_ERROR") {
        console.error("Error subscribing to device state:", error);
      } else {
        console.warn("Device state subscription status:", status);
      }
    });

  return () => {
    supabaseClient.removeChannel(deviceStateChannel);
  };
};
