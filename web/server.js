import express from "express";
import fs from "fs";
import path from "path";
import { fileURLToPath } from "url";
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const app = express();
const PORT = 3000;
app.use(express.static(path.join(__dirname)));
app.get("/frame", (req, res) => {
  const base64Image = fs.readFileSync(
    path.join(__dirname, "sample-frame-base64.txt"),
    "utf-8"
  );
  res.json({ frame: base64Image });
});
app.listen(PORT, () => {
  console.log(`HTTP server running at http://localhost:${PORT}`);
});
