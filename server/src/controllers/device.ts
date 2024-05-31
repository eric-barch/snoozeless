import { Context } from "hono";
import { subscribeToDeviceState } from "@/services/device";
import { streamSSE } from "hono/streaming";

export const getDeviceState = async (c: Context) => {
  return streamSSE(c, async (stream) => {
    const callback = async (deviceStateChange: any) => {
      await stream.writeSSE({
        data: JSON.stringify(deviceStateChange),
      });
    };

    const unsubscribeFromDeviceState = subscribeToDeviceState(callback);

    stream.onAbort(() => {
      console.log("Device state stream aborted.");
      unsubscribeFromDeviceState();
    });

    while (true) {
      /**NOTE: Duration argument does not seem to affect delivery time? */
      await stream.sleep(30000);
    }
  });
};
