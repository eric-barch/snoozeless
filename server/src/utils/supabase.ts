import { SupabaseClient, createClient } from "@supabase/supabase-js";
import { HTTPException } from "hono/http-exception";

const supabaseUrl = process.env.SUPABASE_URL!;
const supabaseKey = process.env.SUPABASE_KEY!;

export const createAuthenticatedClient = async (
  accessToken: string,
  refreshToken: string,
): Promise<SupabaseClient> => {
  const supabaseClient = createClient(supabaseUrl, supabaseKey);

  const { error } = await supabaseClient.auth.setSession({
    access_token: accessToken,
    refresh_token: refreshToken,
  });

  if (error) {
    throw new HTTPException(401, { message: error.message });
  }

  return supabaseClient;
};
