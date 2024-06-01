import { createMiddleware } from "hono/factory";
import { HTTPException } from "hono/http-exception";

const auth = createMiddleware(async (c, next) => {
  const authorizationHeader = c.req.header("Authorization");
  const refreshToken = c.req.header("Refresh-Token");

  if (!authorizationHeader) {
    throw new HTTPException(401, {
      message: "Unauthorized - Authorization Header Missing",
    });
  }

  if (!authorizationHeader.startsWith("Bearer ")) {
    throw new HTTPException(400, {
      message: "Bad Request - Incorrect Authorization Header",
    });
  }

  if (!refreshToken) {
    throw new HTTPException(401, {
      message: "Unauthorized - Refresh Token Missing",
    });
  }

  await next();
});

export { auth };
