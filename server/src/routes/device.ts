import { Hono } from "hono";
import {
  registerDeviceController,
  getDeviceStateController,
  updateDeviceStateController,
} from "@/controllers/device";
import { auth } from "@/middleware/auth";

const deviceRoutes = new Hono();

deviceRoutes.use(auth);

deviceRoutes.post("/register", registerDeviceController);
deviceRoutes.get("/state", getDeviceStateController);
deviceRoutes.patch("/state", updateDeviceStateController);

export { deviceRoutes };
