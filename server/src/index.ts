import "dotenv/config";
import { Hono } from "hono";
import { serve } from "@hono/node-server";
import { deviceRoutes } from "@/routes/device";

const app = new Hono();

app.route("/device", deviceRoutes);

const port = 3000;

serve({
  fetch: app.fetch,
  port,
});

console.log(`Snoozeless server running on port ${port}`);
