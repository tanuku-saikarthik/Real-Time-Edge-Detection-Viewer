const img = document.getElementById("frame") as HTMLImageElement;
const fpsEl = document.getElementById("fps") as HTMLElement;
let last = performance.now();
const updateFPS = () => {
  const now = performance.now();
  const fps = 1000.0 / (now - last);
  last = now;
  fpsEl.innerText = fps.toFixed(1);
  requestAnimationFrame(updateFPS);
};
updateFPS();
const fetchFrame = async () => {
  try {
    const response = await fetch("http://localhost:3000/frame");
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    const data = await response.json();
    const { frame } = data;
    img.src = `data:image/png;base64,${frame}`;
  } catch (err) {
    console.error("Error fetching frame", err);
  }
};
setInterval(fetchFrame, 1000);
