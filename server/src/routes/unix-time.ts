import { Hono } from "hono";
import { auth } from "@/middleware/auth";
import { getUnixTimeController } from "@/controllers/unix-time";

const unixTimeRoutes = new Hono();

unixTimeRoutes.use(auth);

unixTimeRoutes.get("/", getUnixTimeController);

export { unixTimeRoutes };
