export default {
  async fetch(request) {
    return new Response("Bot is alive!", { status: 200 });
  },
};
