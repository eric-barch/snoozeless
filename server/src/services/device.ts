import { jwtDecode } from "jwt-decode";
import { createAuthenticatedClient } from "@/utils/supabase";

type DecodedJwt = {
  sub: string;
  session_id: string;
};

type DeviceStateChangeCallback = (deviceStateChange: any) => void;

export const registerDeviceService = async (
  accessToken: string,
  refreshToken: string,
  deviceState: any,
) => {
  const supabaseClient = createAuthenticatedClient(accessToken, refreshToken);

  const decodedJwt = jwtDecode<DecodedJwt>(accessToken);

  const userId = decodedJwt.sub;
  const sessionId = decodedJwt.session_id;

  const { data, error } = await supabaseClient
    .from("devices")
    .insert({ user_id: userId, session_id: sessionId, ...deviceState })
    .select();

  return { data, error };
};

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
