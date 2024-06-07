import { Context } from "hono";
import { streamSSE } from "hono/streaming";
import { RealtimePostgresUpdatePayload } from "@supabase/supabase-js";
import {
  registerDeviceService,
  unregisterDeviceService,
  getDeviceStateService,
  updateDeviceStateService,
} from "@/services/device";
import { extractAuthTokens } from "@/utils/auth";

export const registerDeviceController = async (c: Context) => {
  const { accessToken, refreshToken } = extractAuthTokens(c);

  const initialState = await c.req.json();

  const { data, error } = await registerDeviceService(
    accessToken,
    refreshToken,
    initialState,
  );

  if (error) {
    return c.json(error, 400);
  }

  return c.json(data, 201);
};

export const unregisterDeviceController = async (c: Context) => {
  const { accessToken, refreshToken } = extractAuthTokens(c);

  const { deviceId } = await c.req.json();

  const { data, error } = await unregisterDeviceService(
    accessToken,
    refreshToken,
    deviceId,
  );

  if (error) {
    return c.json(error, 400);
  }

  return c.json(data, 204);
};

export const getDeviceStateController = async (c: Context) => {
  const { accessToken, refreshToken } = extractAuthTokens(c);

  const deviceId = c.req.query("deviceId");

  if (!deviceId) {
    return c.json(400);
  }

  const response = streamSSE(c, async (stream) => {
    let isStreaming = true;

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
      deviceId,
      callback,
    );

    stream.onAbort(() => {
      isStreaming = false;
      unsubscribeFromDeviceState();
      console.log("Device state stream aborted.");
    });

    while (isStreaming) {
      await stream.sleep(30000);
    }
  });

  /**Hono SSE helper uses HTTP1 headers, which Node HTTP2 secure server
   * doesn't like. Remove all headers here.*/
  Array.from(response.headers.keys()).forEach((header) =>
    response.headers.delete(header),
  );

  return response;
};

export const updateDeviceStateController = async (c: Context) => {
  const { accessToken, refreshToken } = extractAuthTokens(c);

  const { deviceId, stateUpdate } = await c.req.json();

  const { data, error } = await updateDeviceStateService(
    accessToken,
    refreshToken,
    deviceId,
    stateUpdate,
  );

  if (error) {
    return c.json(error, 400);
  }

  return c.json(data, 204);
};
