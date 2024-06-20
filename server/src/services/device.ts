import { createAuthenticatedClient } from "@/utils/supabase";
import { SSEStreamingApi } from "hono/streaming";
import { HTTPException } from "hono/http-exception";
import { Context } from "hono";

export const registerDeviceService = async (c: Context) => {
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
    .select();

  if (insertError) {
    throw new HTTPException(400, { message: insertError.message });
  }

  return insertData;
};

export const unregisterDeviceService = async (
  accessToken: string,
  refreshToken: string,
  deviceId: string,
) => {
  const supabaseClient = await createAuthenticatedClient(
    accessToken,
    refreshToken,
  );

  const { data, error: deleteError } = await supabaseClient
    .from("devices")
    .delete()
    .eq("id", deviceId)
    .select();

  if (deleteError || data.length <= 0) {
    throw new HTTPException(400, { message: deleteError?.message });
  }

  const { error: signOutError } = await supabaseClient.auth.signOut();

  if (signOutError) {
    throw new HTTPException(400, { message: signOutError.message });
  }

  return data;
};

export const getDeviceStateService = async (
  c: Context,
  stream: SSEStreamingApi,
  deviceId: string,
) => {
  const supabaseClient = c.get("supabaseClient");

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
          data: JSON.stringify(deviceStateChange),
        });
      },
    )
    .subscribe((status, error) => {
      if (status === "SUBSCRIBED") {
        console.log("Successfully subscribed to device state updates.");
      } else if (status === "CHANNEL_ERROR") {
        console.error("Channel error:", error);
      } else {
        console.log("Subscription status:", status);
      }
    });

  return () => {
    supabaseClient.removeChannel(deviceStateChannel);
  };
};

export const updateDeviceStateService = async (
  accessToken: string,
  refreshToken: string,
  deviceId: string,
  stateUpdate: any,
) => {
  const supabaseClient = await createAuthenticatedClient(
    accessToken,
    refreshToken,
  );

  const { data, error } = await supabaseClient
    .from("devices")
    .update(stateUpdate)
    .eq("id", deviceId)
    .select();

  if (error) {
    throw new HTTPException(400, { message: error.message });
  }

  return data;
};
