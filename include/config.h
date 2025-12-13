/*
 * Configuration file for Light Detection Alert System
 *
 * IMPORTANT: Copy this file to config.h and update with your actual credentials
 * DO NOT commit config.h with real credentials to version control!
 */

#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Discord Webhook URL
// To create a Discord webhook:
// 1. Open Discord and go to your server
// 2. Click on Server Settings > Integrations > Webhooks
// 3. Click "New Webhook" or "Create Webhook"
// 4. Give it a name and select a channel
// 5. Copy the Webhook URL
// 6. Paste it below (keep the quotes)
#define DISCORD_WEBHOOK_URL "https://discord.com/api/webhooks/YOUR_WEBHOOK_URL_HERE"

// Alternative: Telegram Bot (uncomment if using Telegram instead)
// To create a Telegram bot:
// 1. Message @BotFather on Telegram
// 2. Send /newbot and follow instructions
// 3. Copy the bot token
// 4. Get your chat ID by messaging @userinfobot
// #define TELEGRAM_BOT_TOKEN "YOUR_BOT_TOKEN"
// #define TELEGRAM_CHAT_ID "YOUR_CHAT_ID"

#endif
