/**
  @file locale_pt_br.h

  Brazilian Portuguese locale payload for menu, HUD and story text.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _SFG_LOCALE_PT_BR_H
#define _SFG_LOCALE_PT_BR_H

static const SFG_LevelMeta SFG_levelMeta_pt_br[SFG_NUMBER_OF_LEVELS] = {0};

static const char *const SFG_menuItemTexts_pt_br[] =
{
  "continuar",
  "mapa",
  "jogar",
  "carregar",
  "som fx",
  "musica",
  "camera",
  "idioma",
  "sair"
};

static const SFG_Locale SFG_locale_pt_br =
{
  "pt_BR",
  SFG_menuItemTexts_pt_br,
  "abates",
  "salvar? L nao sim R",
  "salvo",
  "fase concluida",
  "O mundo e absurdo. As maquinas da Macrochip, nascidas da ambicao vazia, "
  "querem impor uma logica fria ao caos da existencia humana. Mas o homem "
  "recusa ser reduzido a algoritmo. Diante dessa tirania esteril, "
  "ha uma resposta: rebeliao.",
  "A logica fria foi silenciada, mas o absurdo do mundo continua. "
  "Nao encontramos salvacao, mas encontramos dignidade na luta. "
  "A luta basta para encher o coracao. Voce segue adiante.",
  "SISTEMA MALICIOSO DETECTADO",
  "ligado",
  "desligado",
  SFG_levelMeta_pt_br
};

#endif
