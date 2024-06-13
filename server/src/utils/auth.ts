import { Context } from "hono";
import { HTTPException } from "hono/http-exception";

export const extractAuthTokens = (c: Context) => {
  const accessHeader = c.req.header("Authorization");
  const refreshToken = c.req.header("Refresh-Token");

  if (!accessHeader) {
    throw new HTTPException(401, {
      message: "Unauthorized - Missing Authorization Header",
    });
  }

  if (!refreshToken) {
    throw new HTTPException(401, {
      message: "Unauthorized - Missing Refresh-Token Header",
    });
  }

  /**Remove "Bearer " prefix. */
  const accessToken = accessHeader.substring(7);

  return { accessToken, refreshToken };
};
