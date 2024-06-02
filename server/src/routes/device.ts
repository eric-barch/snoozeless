import { Hono } from "hono";
import {
  registerDeviceController,
  unregisterDeviceController,
  getDeviceStateController,
  updateDeviceStateController,
} from "@/controllers/device";
import { auth } from "@/middleware/auth";

const deviceRoutes = new Hono();

deviceRoutes.use(auth);

deviceRoutes.post("/register", registerDeviceController);
deviceRoutes.delete("/unregister", unregisterDeviceController);
deviceRoutes.get("/state", getDeviceStateController);
deviceRoutes.patch("/state", updateDeviceStateController);

export { deviceRoutes };
