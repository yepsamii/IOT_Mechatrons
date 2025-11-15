# CI/CD Setup Guide for Vercel Deployment

This guide will help you set up automatic deployments to Vercel whenever you push to the `main` branch.

## Prerequisites

- A Vercel account with your project already deployed
- GitHub repository set up for this project
- Admin access to the GitHub repository

## Step 1: Get Your Vercel Token

1. Go to [Vercel Account Settings](https://vercel.com/account/tokens)
2. Click "Create Token"
3. Give it a name (e.g., "GitHub Actions Deploy")
4. Set the scope to "Full Account"
5. Click "Create" and **copy the token immediately** (you won't be able to see it again)

## Step 2: Get Your Vercel Project IDs

You need two IDs from your Vercel project:

### Method 1: Using Vercel CLI (Recommended)

1. Install Vercel CLI if you haven't already:

   ```bash
   npm install -g vercel
   ```

2. Navigate to the web-app directory:

   ```bash
   cd web-app
   ```

3. Login to Vercel:

   ```bash
   vercel login
   ```

4. Link your project:

   ```bash
   vercel link
   ```

5. After linking, check the `.vercel/project.json` file:

   ```bash
   cat .vercel/project.json
   ```

   You'll see something like:

   ```json
   {
     "orgId": "team_xxxxxxxxxxxxx",
     "projectId": "prj_xxxxxxxxxxxxx"
   }
   ```

### Method 2: From Vercel Dashboard

1. Go to your project on [Vercel Dashboard](https://vercel.com/dashboard)
2. Click on your project
3. Go to Settings → General
4. Copy your **Project ID** (visible on the page)
5. For **Org ID** (Team ID), go to your team/account settings

## Step 3: Add GitHub Secrets

1. Go to your GitHub repository
2. Navigate to **Settings** → **Secrets and variables** → **Actions**
3. Click **"New repository secret"** and add the following three secrets:

   | Secret Name         | Value                         | Description             |
   | ------------------- | ----------------------------- | ----------------------- |
   | `VERCEL_TOKEN`      | Your Vercel token from Step 1 | Authentication token    |
   | `VERCEL_ORG_ID`     | Your org/team ID from Step 2  | Organization identifier |
   | `VERCEL_PROJECT_ID` | Your project ID from Step 2   | Project identifier      |

## Step 4: Commit and Push the CI/CD Files

1. Add the new files to git:

   ```bash
   git add .github/workflows/deploy.yml vercel.json CICD_SETUP.md
   ```

2. Commit the changes:

   ```bash
   git commit -m "Add CI/CD workflow for automatic Vercel deployment"
   ```

3. Push to main:
   ```bash
   git push origin main
   ```

## Step 5: Verify the Deployment

1. Go to your GitHub repository
2. Click on the **"Actions"** tab
3. You should see your workflow running
4. Click on the workflow run to see the deployment progress
5. Once complete, your changes will be live on Vercel!

## How It Works

The CI/CD pipeline is configured to:

1. **Trigger**: Automatically runs on every push to the `main` branch
2. **Build**: Installs dependencies and builds your Vite application
3. **Deploy**: Deploys the built artifacts to Vercel production

## Troubleshooting

### Workflow fails with "Invalid token"

- Double-check that your `VERCEL_TOKEN` secret is correct
- Make sure the token hasn't expired
- Regenerate a new token if needed

### Workflow fails with "Project not found"

- Verify `VERCEL_PROJECT_ID` matches your project
- Verify `VERCEL_ORG_ID` is correct
- Make sure the project exists in your Vercel dashboard

### Build fails

- Check the workflow logs in GitHub Actions
- Ensure your app builds successfully locally with `npm run build`
- Verify all dependencies are listed in `package.json`

### Deployment succeeds but site doesn't update

- Clear your browser cache
- Wait a few minutes for CDN propagation
- Check Vercel dashboard for deployment status

## Additional Configuration

### Environment Variables

If your app uses environment variables:

1. Add them in Vercel Dashboard → Project Settings → Environment Variables
2. Or use `vercel env` command to manage them

### Custom Domain

Your custom domain settings in Vercel will continue to work automatically with this CI/CD setup.

## Testing Before Merging to Main

If you want to test deployments on a preview branch:

1. Create a feature branch:

   ```bash
   git checkout -b feature/my-feature
   ```

2. Push changes:

   ```bash
   git push origin feature/my-feature
   ```

3. Vercel will automatically create a preview deployment
4. Once tested, merge to main for production deployment

---

**Need Help?**

- [Vercel Documentation](https://vercel.com/docs)
- [GitHub Actions Documentation](https://docs.github.com/en/actions)
