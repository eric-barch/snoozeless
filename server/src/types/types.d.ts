/**See https://hono.dev/docs/api/context#contextvariablemap */

import { SupabaseClient } from "@supabase/supabase-js";

declare module "hono" {
  interface ContextVariableMap {
    supabaseClient: SupabaseClient;
  }
}
