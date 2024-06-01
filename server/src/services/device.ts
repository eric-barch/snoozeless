import { createAuthenticatedClient } from "@/utils/supabase";

type DeviceStateChangeCallback = (deviceStateChange: any) => void;

export const registerDeviceService = async (
  accessToken: string,
  refreshToken: string,
  deviceState: any,
) => {
  const supabaseClient = createAuthenticatedClient(accessToken, refreshToken);

  const { data: userData, error: userError } =
    await supabaseClient.auth.getUser();

  if (userError) {
    return { data: userData, error: userError };
  }

  const userId = userData.user.id;

  const { data: deviceData, error: deviceError } = await supabaseClient
    .from("devices")
    .insert({ user_id: userId, ...deviceState })
    .select();

  if (deviceError) {
    console.log("user", userData);
    console.log("error", deviceError);
  }

  return { data: deviceData, error: deviceError };
};

export const getDeviceStateService = async (
  accessToken: string,
  refreshToken: string,
  callback: DeviceStateChangeCallback,
) => {
  const supabaseClient = createAuthenticatedClient(accessToken, refreshToken);

  const deviceStateChannel = supabaseClient
    .channel("device-state-changes")
    .on(
      "postgres_changes",
      {
        event: "UPDATE",
        schema: "public",
        table: "devices",
      },
      (deviceStateChange) => {
        callback(deviceStateChange);
      },
    )
    .subscribe((status, error) => {
      if (status === "SUBSCRIBED") {
        console.log("Successfully subscribed to device state changes.");
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
