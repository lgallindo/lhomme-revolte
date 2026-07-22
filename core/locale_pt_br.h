/**
  @file locale_pt_br.h

  Brazilian Portuguese locale payload for menu, HUD and story text.

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef _LHR_LOCALE_PT_BR_H
#define _LHR_LOCALE_PT_BR_H

static const LHR_LevelMeta LHR_levelMeta_pt_br[LHR_NUMBER_OF_LEVELS] = {0};

static const char *const LHR_menuItemTexts_pt_br[] =
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

static const LHR_Locale LHR_locale_pt_br =
{
  "pt_BR",
  LHR_menuItemTexts_pt_br,
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
  LHR_levelMeta_pt_br
};

#endif
