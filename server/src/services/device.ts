import { createClient } from "@supabase/supabase-js";

const supabaseUrl = process.env.SUPABASE_URL!;
const supabaseKey = process.env.SUPABASE_KEY!;
const supabaseClient = createClient(supabaseUrl, supabaseKey);

type DeviceStateChangeCallback = (deviceStateChange: any) => void;

export const subscribeToDeviceState = (callback: DeviceStateChangeCallback) => {
  const deviceStateChannel = supabaseClient
    .channel("device-state-changes")
    .on(
      "postgres_changes",
      {
        event: "UPDATE",
        schema: "public",
        table: "devices",
      },
      (payload) => {
        callback(payload);
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
