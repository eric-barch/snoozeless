import { Hono } from "hono";
import { auth } from "@/middleware/auth";
import { refreshAuthController } from "@/controllers/auth";

const authRoutes = new Hono();

authRoutes.use(auth);

authRoutes.post("/refresh", refreshAuthController);

export { authRoutes };
