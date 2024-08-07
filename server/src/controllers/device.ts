import { Context } from "hono";
import { streamSSE } from "hono/streaming";
import {
  postDeviceEnrollService,
  getDeviceStateService,
  postDeviceAlarmService,
} from "@/services/device";
import { HTTPException } from "hono/http-exception";

export const postDeviceEnrollController = async (
  c: Context,
): Promise<Response> => {
  const response = await postDeviceEnrollService(c);
  return c.json(response, 201);
};

export const getDeviceStateController = async (
  c: Context,
): Promise<Response> => {
  const deviceId = c.req.query("deviceId");

  if (!deviceId) {
    throw new HTTPException(400, {
      message: "Bad Request - Missing required query parameter: deviceId",
    });
  }

  const response = streamSSE(c, async (stream) => {
    let isStreaming = true;

    const unsubscribeFromDeviceState = await getDeviceStateService(
      c,
      deviceId,
      stream,
    );

    stream.onAbort(() => {
      isStreaming = false;
      unsubscribeFromDeviceState();
      console.log("Device state stream aborted.");
    });

    while (isStreaming) {
      await stream.sleep(60000);
      await stream.writeSSE({ event: "keep-alive", data: "keep-alive" });
    }
  });

  /**Hono SSE helper uses HTTP1 headers, which Node HTTP2 secure server
   * doesn't like. Remove all headers here.*/
  Array.from(response.headers.keys()).forEach((header) =>
    response.headers.delete(header),
  );

  return response;
};

export const postDeviceAlarmController = async (
  c: Context,
): Promise<Response> => {
  const deviceId = c.req.query("deviceId");

  if (!deviceId) {
    throw new HTTPException(400, {
      message: "Bad Request - Missing required query parameter: deviceId",
    });
  }

  let alarm = {};
  try {
    const parsedBody = await c.req.json();
    if (parsedBody) {
      alarm = parsedBody;
    }
  } catch {
    /**`alarm` remains an empty object. */
  }

  const response = await postDeviceAlarmService(c, deviceId, alarm);
  return c.json(response, 201);
};
