# 🚀 Quick Deployment Setup

## What You Need

1. **Vercel Token** - Get from: https://vercel.com/account/tokens
2. **Project ID** - Run: `cd web-app && vercel link` then `cat .vercel/project.json`
3. **Org ID** - Same file as above

## Add to GitHub Secrets

Go to: **Your Repo → Settings → Secrets and variables → Actions**

Add these 3 secrets:
- `VERCEL_TOKEN`
- `VERCEL_PROJECT_ID`
- `VERCEL_ORG_ID`

## That's It!

Now every push to `main` will automatically deploy to Vercel! 🎉

View deployment status: **Your Repo → Actions tab**

---

**Full Guide:** See `CICD_SETUP.md` in the root directory

