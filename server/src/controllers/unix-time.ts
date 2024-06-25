import { getUnixTimeService } from "@/services/unix-time";
import { Context } from "hono";

export const getUnixTimeController = async (c: Context): Promise<Response> => {
  const unixTime = await getUnixTimeService();
  const response = { unix_time: unixTime };
  return c.json(response, 200);
};
