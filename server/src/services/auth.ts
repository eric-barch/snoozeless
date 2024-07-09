import { Context } from "hono";
import { HTTPException } from "hono/http-exception";

export const refreshAuthService = async (c: Context) => {
  const supabaseClient = c.get("supabaseClient");

  const { data: sessionData, error: sessionError } =
    await supabaseClient.auth.refreshSession();

  if (sessionError) {
    throw new HTTPException(400, { message: sessionError.message });
  }

  const session = sessionData.session;

  if (!session) {
    throw new HTTPException(400, { message: "Session not found." });
  }

  const { access_token: accessToken, refresh_token: refreshToken } = session;

  return { accessToken, refreshToken };
};
