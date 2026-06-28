#!/bin/bash
set -e

# Create directories
mkdir -p docs/vendor/fonts
mkdir -p docs/vendor/images

echo "Downloading Inter fonts..."
curl -L -o docs/vendor/fonts/inter-300.ttf "https://fonts.gstatic.com/s/inter/v20/UcCO3FwrK3iLTeHuS_nVMrMxCp50SjIw2boKoduKmMEVuOKfMZg.ttf"
curl -L -o docs/vendor/fonts/inter-400.ttf "https://fonts.gstatic.com/s/inter/v20/UcCO3FwrK3iLTeHuS_nVMrMxCp50SjIw2boKoduKmMEVuLyfMZg.ttf"
curl -L -o docs/vendor/fonts/inter-500.ttf "https://fonts.gstatic.com/s/inter/v20/UcCO3FwrK3iLTeHuS_nVMrMxCp50SjIw2boKoduKmMEVuI6fMZg.ttf"
curl -L -o docs/vendor/fonts/inter-600.ttf "https://fonts.gstatic.com/s/inter/v20/UcCO3FwrK3iLTeHuS_nVMrMxCp50SjIw2boKoduKmMEVuGKYMZg.ttf"

echo "Downloading Outfit fonts..."
curl -L -o docs/vendor/fonts/outfit-400.ttf "https://fonts.gstatic.com/s/outfit/v15/QGYyz_MVcBeNP4NjuGObqx1XmO1I4TC1C4E.ttf"
curl -L -o docs/vendor/fonts/outfit-600.ttf "https://fonts.gstatic.com/s/outfit/v15/QGYyz_MVcBeNP4NjuGObqx1XmO1I4e6yC4E.ttf"
curl -L -o docs/vendor/fonts/outfit-800.ttf "https://fonts.gstatic.com/s/outfit/v15/QGYyz_MVcBeNP4NjuGObqx1XmO1I4bCyC4E.ttf"

echo "Downloading Wikimedia images..."
curl -L -o docs/vendor/images/Anarcho__animation_2_inverted.gif "https://upload.wikimedia.org/wikipedia/commons/e/e1/Anarcho__animation_2_inverted.gif"
curl -L -o docs/vendor/images/Anarch_Devices.jpg "https://upload.wikimedia.org/wikipedia/commons/8/83/Anarch_Devices.jpg"

echo "Downloading Aileron fonts..."
curl -L -o docs/vendor/fonts/Aileron-Light.otf "https://raw.githubusercontent.com/reinhart1010/aileron/master/fonts/Aileron-Light.otf"
curl -L -o docs/vendor/fonts/Aileron-Bold.otf "https://raw.githubusercontent.com/reinhart1010/aileron/master/fonts/Aileron-Bold.otf"
curl -L -o docs/vendor/fonts/Aileron-LightItalic.otf "https://raw.githubusercontent.com/reinhart1010/aileron/master/fonts/Aileron-LightItalic.otf"

echo "All downloads completed successfully."
