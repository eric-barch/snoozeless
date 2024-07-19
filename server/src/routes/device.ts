import { Hono } from "hono";
import { auth } from "@/middleware/auth";
import {
  enrollDeviceController,
  getDeviceStateController,
} from "@/controllers/device";

const deviceRoutes = new Hono();

deviceRoutes.use(auth);

deviceRoutes.post("/enroll", enrollDeviceController);
deviceRoutes.get("/state", getDeviceStateController);

export { deviceRoutes };
