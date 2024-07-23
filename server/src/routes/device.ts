import { Hono } from "hono";
import { auth } from "@/middleware/auth";
import {
  postDeviceEnrollController,
  getDeviceStateController,
  postDeviceAlarmController,
} from "@/controllers/device";

const deviceRoutes = new Hono();

deviceRoutes.use(auth);

deviceRoutes.post("/enroll", postDeviceEnrollController);
deviceRoutes.get("/state", getDeviceStateController);
deviceRoutes.post("/alarm", postDeviceAlarmController);

export { deviceRoutes };
