import { Hono } from "hono";
import { getDeviceState } from "@/controllers/device";

const deviceRoutes = new Hono();

deviceRoutes.get("/state", getDeviceState);

export { deviceRoutes };
