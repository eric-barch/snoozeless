import { Context } from "hono";
import { subscribeToDeviceState } from "@/services/device";
import { streamSSE } from "hono/streaming";
import { RealtimePostgresUpdatePayload } from "@supabase/supabase-js";

export const getDeviceState = async (c: Context) => {
  const authorizationHeader = c.req.header("Authorization");
  const refreshToken = c.req.header("Refresh-Token");

  if (!authorizationHeader || !authorizationHeader.startsWith("Bearer ")) {
    return c.json(
      { error: "Unauthorized - Authorization Header Missing" },
      401,
    );
  }

  if (!refreshToken) {
    return c.json({ error: "Unauthorized - Refresh Token Missing" }, 401);
  }

  /** Remove "Bearer " prefix. */
  const accessToken = authorizationHeader.substring(7);

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
