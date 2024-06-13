import { Context } from "hono";
import { streamSSE } from "hono/streaming";
import {
  registerDeviceService,
  unregisterDeviceService,
  getDeviceStateService,
  updateDeviceStateService,
} from "@/services/device";
import { extractAuthTokens } from "@/utils/auth";
import { HTTPException } from "hono/http-exception";

export const registerDeviceController = async (
  c: Context,
): Promise<Response> => {
  const { accessToken, refreshToken } = extractAuthTokens(c);

  const initialState = await c.req.json();

  const data = await registerDeviceService(
    accessToken,
    refreshToken,
    initialState,
  );

  return c.json(data, 201);
};

export const unregisterDeviceController = async (
  c: Context,
): Promise<Response> => {
  const { accessToken, refreshToken } = extractAuthTokens(c);

  const { deviceId } = await c.req.json();

  const data = await unregisterDeviceService(
    accessToken,
    refreshToken,
    deviceId,
  );

  return c.json(data, 204);
};

export const getDeviceStateController = async (
  c: Context,
): Promise<Response> => {
  const { accessToken, refreshToken } = extractAuthTokens(c);

  const deviceId = c.req.query("deviceId");

  if (!deviceId) {
    throw new HTTPException(400, {
      message: "Bad Request - Missing required query parameter: deviceId",
    });
  }

  const response = streamSSE(c, async (stream) => {
    let isStreaming = true;

    const unsubscribeFromDeviceState = await getDeviceStateService(
      stream,
      accessToken,
      refreshToken,
      deviceId,
    );

    stream.onAbort(() => {
      isStreaming = false;
      unsubscribeFromDeviceState();
      console.log("Device state stream aborted.", new Date().toLocaleString());
    });

    while (isStreaming) {
      await stream.writeSSE({ data: "keep-alive" });
      await stream.sleep(60000);
    }
  });

  /**Hono SSE helper uses HTTP1 headers, which Node HTTP2 secure server
   * doesn't like. Remove all headers here.*/
  Array.from(response.headers.keys()).forEach((header) =>
    response.headers.delete(header),
  );

  return response;
};

export const updateDeviceStateController = async (
  c: Context,
): Promise<Response> => {
  const { accessToken, refreshToken } = extractAuthTokens(c);

  const { deviceId, stateUpdate } = await c.req.json();

  const data = await updateDeviceStateService(
    accessToken,
    refreshToken,
    deviceId,
    stateUpdate,
  );

  return c.json(data, 204);
};
