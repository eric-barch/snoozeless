import { Context } from "hono";
import { refreshAuthService } from "@/services/auth";

export const refreshAuthController = async (c: Context): Promise<Response> => {
  const response = await refreshAuthService(c);
  return c.json(response, 201);
};
