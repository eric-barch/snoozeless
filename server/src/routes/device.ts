import { Hono } from "hono";
import { auth } from "@/middleware/auth";
import {
  registerDeviceController,
  getDeviceStateController,
  updateDeviceStateController,
} from "@/controllers/device";

const deviceRoutes = new Hono();

deviceRoutes.use(auth);

deviceRoutes.post("/register", registerDeviceController);
deviceRoutes.get("/state", getDeviceStateController);
deviceRoutes.patch("/state", updateDeviceStateController);

export { deviceRoutes };
