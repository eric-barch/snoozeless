import { Context } from "hono";
import { HTTPException } from "hono/http-exception";

export const refreshAuthService = async (c: Context) => {
  const supabaseClient = c.get("supabaseClient");

  const { data, error } = await supabaseClient.auth.refreshSession();

  if (error) {
    console.error("Error refreshing session: ", error);
    throw new HTTPException(400, { message: error.message });
  }

  const session = data.session;

  if (!session) {
    console.error("Session not found.");
    throw new HTTPException(400, { message: "Session not found." });
  }

  const { access_token, refresh_token } = session;

  return { access_token, refresh_token };
};
