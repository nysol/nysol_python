/* ////////// LICENSE INFO ////////////////////

 * Copyright (C) 2013 by NYSOL CORPORATION
 *
 * Unless you have received this program directly from NYSOL pursuant
 * to the terms of a commercial license agreement with NYSOL, then
 * this program is licensed to you under the terms of the GNU Affero General
 * Public License (AGPL) as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THOSE OF 
 * NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Please refer to the AGPL (http://www.gnu.org/licenses/agpl-3.0.txt)
 * for more details.

 ////////// LICENSE INFO ////////////////////*/
// =============================================================================
/// kgkmeansHelp.h : kgedist help
// =============================================================================
_title="k-mean法によるクラスタリング";
_doc="\
\n\
数値属性に基づいて、k-mean法によりクラスタリングをおこない、そのクラスタ番号を出力する。\n\
\n\
書式\n\
mkmeans f= n= a= [d=] [m=] [S=] [-null] [-nfn] [-nfno] [-x][tmpPath=]\n\
[precision=] [--help] [--helpl] [--version]\n\
\n\
パラメータ\n\
\n\
  S=   乱数の種【デフォルト値:現在時刻】\n\
  a=   追加する項目名(クラスタ番号に対する項目名)\n\
  d=   初期シードの計算方法【デフォルト値:2】\n\
       0:random 1:Kaufman Approach 2:Bradley & Fayyad Approach\n\
  f=   ここで指定された項目でクラスタリングを行う\n\
  m=   Bradley & Fayyad Approach利用時に用意するサンプル数【デフォルト値:10】\n\
  n=   クラスタ数\n\
	-null 一つでもNULLだとクラスタ番号をNULLとする\n\
";



