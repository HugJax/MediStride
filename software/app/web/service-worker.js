// MediStride — Service worker minimal (offline-first pour l'app statique)
const CACHE = 'medistride-v1';
const ASSETS = [
    './',
    './index.html',
    './flash.html',
    './css/style.css',
    './js/app.js',
    './js/ble.js',
    './js/viz.js',
    './js/analysis.js',
    './js/export.js',
    './manifest.webmanifest',
    './icons/icon.svg'
];

self.addEventListener('install', (event) => {
    event.waitUntil(
        caches.open(CACHE).then((cache) => cache.addAll(ASSETS)).catch(() => {})
    );
    self.skipWaiting();
});

self.addEventListener('activate', (event) => {
    event.waitUntil(
        caches.keys().then((keys) =>
            Promise.all(keys.filter(k => k !== CACHE).map(k => caches.delete(k)))
        )
    );
    self.clients.claim();
});

self.addEventListener('fetch', (event) => {
    if (event.request.method !== 'GET') return;
    event.respondWith(
        caches.match(event.request).then((cached) =>
            cached || fetch(event.request).catch(() => cached)
        )
    );
});
