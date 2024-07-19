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
    .channel("device-state-updates")
    .on(
      "postgres_changes",
      {
        event: "UPDATE",
        schema: "public",
        table: "devices",
        filter: `id=eq.${deviceId}`,
      },
      async (deviceStateChange) => {
        await stream.writeSSE({
          event: "device-state-update",
          data: JSON.stringify(deviceStateChange.new),
        });
      },
    )
    .subscribe(async (status, error) => {
      if (status === "SUBSCRIBED") {
        const { data, error } = await supabaseClient
          .from("devices")
          .select("*")
          .eq("id", deviceId)
          .single();

        if (error) {
          await stream.writeSSE({
            event: "device-state-error",
            data: JSON.stringify(error),
          });
        } else {
          await stream.writeSSE({
            event: "initial-device-state",
            data: JSON.stringify(data),
          });
        }
      } else if (status === "CHANNEL_ERROR") {
        console.error("Device state channel error:", error);
      } else {
        console.log("Device state subscription status:", status);
      }
    });

  const deviceAlarmsChannel = supabaseClient
    .channel("device-alarm-changes")
    .on(
      "postgres_changes",
      {
        event: "*",
        schema: "public",
        table: "alarms",
        filter: `device_id=eq.${deviceId}`,
      },
      async (deviceAlarmsChange) => {
        await stream.writeSSE({
          event: "device-alarm-change",
          data: JSON.stringify(deviceAlarmsChange.new),
        });
      },
    )
    .subscribe(async (status, error) => {
      if (status === "SUBSCRIBED") {
        const { data, error } = await supabaseClient
          .from("alarms")
          .select("*")
          .eq("device_id", deviceId);

        if (error) {
          await stream.writeSSE({
            event: "device-alarms-error",
            data: JSON.stringify(error),
          });
        } else {
          await stream.writeSSE({
            event: "initial-device-alarms",
            data: JSON.stringify(data),
          });
        }
      } else if (status === "CHANNEL_ERROR") {
        console.error("Device alarms channel error:", error);
      } else {
        console.log("Device alarms subscription status:", status);
      }
    });

  return () => {
    supabaseClient.removeChannel(deviceStateChannel);
    supabaseClient.removeChannel(deviceAlarmsChannel);
  };
};
