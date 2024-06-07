import "dotenv/config";
import { readFileSync } from "fs";
import { logger } from "hono/logger";
import { createSecureServer } from "http2";
import { Hono } from "hono";
import { serve } from "@hono/node-server";
import { deviceRoutes } from "@/routes/device";

const app = new Hono();

app.use(logger());

app.get("/", (c) => {
  return c.text("Snoozeless server working.");
});

app.route("/device", deviceRoutes);

const port = 3000;

const key = readFileSync(process.env.TLS_KEY_FILE!);
const cert = readFileSync(process.env.TLS_CERT_FILE!);

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
