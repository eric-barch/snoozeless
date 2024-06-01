import { Hono } from "hono";
import {
  registerDeviceController,
  getDeviceStateController,
} from "@/controllers/device";
import { auth } from "@/middleware/auth";

const deviceRoutes = new Hono();

deviceRoutes.use(auth);

deviceRoutes.post("/register", registerDeviceController);
deviceRoutes.get("/state", getDeviceStateController);

export { deviceRoutes };
