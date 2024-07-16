import "dotenv/config";
import { readFileSync } from "fs";
import { createSecureServer } from "http2";
import { Hono } from "hono";
import { logger } from "hono/logger";
import { serve } from "@hono/node-server";
import { authRoutes } from "./routes/auth";
import { unixTimeRoutes } from "@/routes/unix-time";
import { deviceRoutes } from "@/routes/device";

const port = 3000;
const key = readFileSync(process.env.TLS_KEY_FILE!);
const cert = readFileSync(process.env.TLS_CERT_FILE!);

const app = new Hono();

app.use(logger());

app.get("/", (c) => {
  return c.text("Snoozeless server OK.");
});

app.route("/auth", authRoutes);
app.route("/unix-time", unixTimeRoutes);
app.route("/device", deviceRoutes);

serve({
  fetch: app.fetch,
  port,
  createServer: createSecureServer,
  serverOptions: {
    key,
    cert,
    allowHTTP1: true,
  },
});

console.log(`Snoozeless server running on port ${port}`);
