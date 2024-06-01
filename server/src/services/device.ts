import { createAuthenticatedClient } from "@/utils/supabase";

type DeviceStateChangeCallback = (deviceStateChange: any) => void;

export const subscribeToDeviceState = async (
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
