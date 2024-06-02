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
  initialState: any,
) => {
  const supabaseClient = createAuthenticatedClient(accessToken, refreshToken);

  const decodedJwt = jwtDecode<DecodedJwt>(accessToken);

  const userId = decodedJwt.sub;
  const sessionId = decodedJwt.session_id;

  const { data, error } = await supabaseClient
    .from("devices")
    .insert({ user_id: userId, session_id: sessionId, ...initialState })
    .select();

  return { data, error };
};

export const unregisterDeviceService = async (
  accessToken: string,
  refreshToken: string,
  deviceId: string,
) => {
  const supabaseClient = createAuthenticatedClient(accessToken, refreshToken);

  const { data: deleteData, error: deleteError } = await supabaseClient
    .from("devices")
    .delete()
    .eq("id", deviceId)
    .select();

  if (deleteError || deleteData.length <= 0) {
    return { data: deleteData, error: deleteError };
  }

  const { error: signOutError } = await supabaseClient.auth.signOut();

  return { data: deleteData, error: signOutError };
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
