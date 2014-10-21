/*
    MX interface

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __MX_H_INCLUDED__
#define __MX_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#define DLL_EXPORT

/*  Handle DSO symbol visibility                                             */
#if defined _WIN32
#   if defined DLL_EXPORT
#       define MX_EXPORT __declspec(dllexport)
#   else
#       define MX_EXPORT __declspec(dllimport)
#   endif
#else
#   if defined __SUNPRO_C  || defined __SUNPRO_CC
#       define MX_EXPORT __global
#   elif (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define MX_EXPORT __attribute__ ((visibility("default")))
#   else
#       define MX_EXPORT
#   endif
#endif

MX_EXPORT int register_info(const char *path);

MX_EXPORT void *load_map_file(const char *filename,int width, int height);

MX_EXPORT void render_agg(void *map,unsigned char &buffer);

MX_EXPORT int map_destroy(void *map);

#undef MX_EXPORT

#ifdef __cplusplus
}
#endif

#endif