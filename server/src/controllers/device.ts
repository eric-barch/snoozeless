import { Context } from "hono";
import {
  registerDeviceService,
  getDeviceStateService,
} from "@/services/device";
import { streamSSE } from "hono/streaming";
import { RealtimePostgresUpdatePayload } from "@supabase/supabase-js";
import { HTTPException } from "hono/http-exception";

const extractAuthTokens = (c: Context) => {
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

  return { accessToken, refreshToken };
};

export const registerDeviceController = async (c: Context) => {
  const { accessToken, refreshToken } = extractAuthTokens(c);

  const deviceState = await c.req.json();

  const { data, error } = await registerDeviceService(
    accessToken,
    refreshToken,
    deviceState,
  );

  if (error) {
    return c.json(error, 400);
  }

  return c.json(data, 201);
};

export const getDeviceStateController = async (c: Context) => {
  const { accessToken, refreshToken } = extractAuthTokens(c);

  return streamSSE(c, async (stream) => {
    const callback = async (
      deviceStateChange: RealtimePostgresUpdatePayload<{ [key: string]: any }>,
    ) => {
      await stream.writeSSE({
        data: JSON.stringify(deviceStateChange),
      });
    };

    const unsubscribeFromDeviceState = await getDeviceStateService(
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
