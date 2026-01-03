import express from 'express';
import dotenv from 'dotenv';
import cron from 'node-cron';
import { webcrypto } from 'node:crypto';
import worker from './index.js';

// Ensure crypto is available globally for Node.js versions < 19
if (!globalThis.crypto) {
  globalThis.crypto = webcrypto;
}

dotenv.config();

const app = express();
const port = process.env.PORT || 3000;

// Middleware to handle raw body for signature verification
app.use(express.text({ type: '*/*' }));

app.all('*', async (req, res) => {
  const protocol = req.headers['x-forwarded-proto'] || req.protocol;
  const host = req.headers['host'];
  const url = `${protocol}://${host}${req.originalUrl}`;
  
  const request = new Request(url, {
    method: req.method,
    headers: req.headers,
    body: ['GET', 'HEAD'].includes(req.method) ? undefined : req.body,
  });

  const env = process.env;
  const ctx = {
    waitUntil: (promise) => {
      // In Node.js, we don't need to do anything special for waitUntil
      // but we should catch errors to avoid unhandled rejections.
      promise.catch(err => console.error('Error in waitUntil:', err));
    }
  };

  try {
    const response = await worker.fetch(request, env, ctx);
    
    res.status(response.status);
    response.headers.forEach((value, key) => {
      res.setHeader(key, value);
    });
    
    const contentType = response.headers.get('content-type');
    if (contentType && contentType.includes('application/json')) {
      const json = await response.json();
      res.json(json);
    } else {
      const body = await response.text();
      res.send(body);
    }
  } catch (error) {
    console.error('Worker error:', error);
    res.status(500).json({ ok: false, error: 'Internal Server Error', detail: error.message });
  }
});

// Scheduled tasks (every minute as requested)
cron.schedule('* * * * *', async () => {
  console.log('Running scheduled task...');
  const env = process.env;
  const ctx = {
    waitUntil: (promise) => {
      promise.catch(err => console.error('Error in scheduled waitUntil:', err));
    }
  };
  try {
    await worker.scheduled({ cron: '* * * * *' }, env, ctx);
  } catch (error) {
    console.error('Scheduled task error:', error);
  }
});

app.listen(port, () => {
  console.log(`Bot server listening at http://localhost:${port}`);
});
