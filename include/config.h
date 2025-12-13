/*
 * Configuration file for Light Detection Alert System
 *
 * IMPORTANT: Copy this file to config.h and update with your actual credentials
 * DO NOT commit config.h with real credentials to version control!
 */

#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
#define WIFI_SSID "HAMOSHAVA"
#define WIFI_PASSWORD "saloniki"

// Discord Webhook URL
// To create a Discord webhook:
// 1. Open Discord and go to your server
// 2. Click on Server Settings > Integrations > Webhooks
// 3. Click "New Webhook" or "Create Webhook"
// 4. Give it a name and select a channel
// 5. Copy the Webhook URL
// 6. Paste it below (keep the quotes)
#define DISCORD_WEBHOOK_URL "https://discord.com/api/webhooks/1449488126631219312/MCicDOREpXB90JH-FNTrfhb9QmarS2XkVtpRTzVapH9ZlpqoxTq9SNviN6JQ4SA0Chkb"

// Alternative: Telegram Bot (uncomment if using Telegram instead)
// To create a Telegram bot:
// 1. Message @BotFather on Telegram
// 2. Send /newbot and follow instructions
// 3. Copy the bot token
// 4. Get your chat ID by messaging @userinfobot
// #define TELEGRAM_BOT_TOKEN "YOUR_BOT_TOKEN"
// #define TELEGRAM_CHAT_ID "YOUR_CHAT_ID"

#endif
