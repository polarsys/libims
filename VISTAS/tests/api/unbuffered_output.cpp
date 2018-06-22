/*******************************************************************************
 * Copyright (c) 2018 Airbus Operations S.A.S                                  *
 *                                                                             *
 * This program and the accompanying materials are made available under the    *
 * terms of the Eclipse Public License v. 2.0 which is available at            *
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License   *
 * v. 1.0 which is available at                                                *
 * http://www.eclipse.org/org/documents/edl-v10.php.                           *
 *                                                                             *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause                            *
 *******************************************************************************/

/*
 * Force autoflush on child process
 * When we fork(), stdout become bufferised even if we 'bufferise' it.
 * then, stdout is displayed only when buffer is full or child process call
 * 'flush'. stdout and stderr become unsyncronized.
 * To correct this, child process have to set stdout unbufferised itself.
 * Whe use for that LD_PRELOAD and a lib with a constructor who do this.
 *
 * usage : LD_PRELOAD='unbuffered_output.so' command
*/
#include <stdio.h>

__attribute__((constructor))void _unbuf_stdout_ctor_()
{
    setvbuf(stdout, NULL, _IOLBF, 0);
}
