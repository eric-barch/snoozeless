import { createClient, SupabaseClient } from "@supabase/supabase-js";

const supabaseUrl = process.env.SUPABASE_URL!;
const supabaseKey = process.env.SUPABASE_KEY!;

export const createAuthenticatedClient = (
  accessToken: string,
  refreshToken: string,
): SupabaseClient => {
  const supabaseClient = createClient(supabaseUrl, supabaseKey);

  supabaseClient.auth.setSession({
    access_token: accessToken,
    refresh_token: refreshToken,
  });

  return supabaseClient;
};
