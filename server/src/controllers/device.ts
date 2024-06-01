import { Context } from "hono";
import { subscribeToDeviceState } from "@/services/device";
import { streamSSE } from "hono/streaming";
import { RealtimePostgresUpdatePayload } from "@supabase/supabase-js";
import { HTTPException } from "hono/http-exception";

export const getDeviceState = async (c: Context) => {
  const accessHeader = c.req.header("Authorization");
  const refreshToken = c.req.header("Refresh-Token");

  if (!accessHeader) {
    throw new HTTPException(401, {
      message: "Unauthorized - Authorization Header Missing",
    });
  }

  if (!refreshToken) {
    throw new HTTPException(401, {
      message: "Unauthorized - Refresh Header Missing",
    });
  }

  /**Remove "Bearer " prefix. */
  const accessToken = accessHeader.substring(7);

  return streamSSE(c, async (stream) => {
    const callback = async (
      deviceStateChange: RealtimePostgresUpdatePayload<{ [key: string]: any }>,
    ) => {
      await stream.writeSSE({
        data: JSON.stringify(deviceStateChange),
      });
    };

    const unsubscribeFromDeviceState = await subscribeToDeviceState(
      accessToken,
      refreshToken,
      callback,
    );

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
