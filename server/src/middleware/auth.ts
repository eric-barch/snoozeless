import { createMiddleware } from "hono/factory";
import { HTTPException } from "hono/http-exception";
import { createAuthenticatedClient } from "@/utils/supabase";

const auth = createMiddleware(async (c, next) => {
  const accessHeader = c.req.header("Authorization");
  const refreshToken = c.req.header("Refresh-Token");

  if (!accessHeader || !accessHeader.startsWith("Bearer ")) {
    throw new HTTPException(401, {
      message: "Unauthorized - Missing or Incorrect Authorization Header",
    });
  }

  const accessToken = accessHeader.split("Bearer ")[1];

  if (!refreshToken) {
    throw new HTTPException(401, {
      message: "Unauthorized - Missing Refresh Token",
    });
  }

  const supabaseClient = await createAuthenticatedClient(
    accessToken,
    refreshToken,
  );

  c.set("supabaseClient", supabaseClient);

  await next();
});

export { auth };
