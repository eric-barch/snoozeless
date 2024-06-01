import { Hono } from "hono";
import { getDeviceState } from "@/controllers/device";
import { auth } from "@/middleware/auth";

const deviceRoutes = new Hono();

deviceRoutes.use(auth);

deviceRoutes.get("/state", getDeviceState);

export { deviceRoutes };
