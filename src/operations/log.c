/**
 * This code is responsible for handling all installation log file operations
 * including initialization, writing step headers, and reading log lines.
 */

#include "../all.h"

void init_install_log(void)
{
    FILE *f = fopen(INSTALL_LOG_PATH, "w");
    if (f)
    {
        fclose(f);
    }
}

void write_install_log_header(const char *step_name)
{
    FILE *f = fopen(INSTALL_LOG_PATH, "a");
    if (f)
    {
        fprintf(f, "\n");
        fprintf(f, "--------------------------------------------------------------\n");
        fprintf(f, "  %s\n", step_name);
        fprintf(f, "--------------------------------------------------------------\n");
        fprintf(f, "\n");
        fclose(f);
    }
}

char **read_install_log_lines(int max_lines, int *out_count)
{
    *out_count = 0;

    FILE *f = fopen(INSTALL_LOG_PATH, "r");
    if (!f)
    {
        return NULL;
    }

    char **lines = calloc(max_lines, sizeof(char *));
    if (!lines)
    {
        fclose(f);
        return NULL;
    }

    char line_buf[512];
    int line_count = 0;

    while (fgets(line_buf, sizeof(line_buf), f))
    {
        // Remove trailing newline.
        size_t len = strlen(line_buf);
        if (len > 0 && line_buf[len - 1] == '\n')
        {
            line_buf[len - 1] = '\0';
        }

        // Circular buffer: free oldest line if buffer is full.
        if (line_count >= max_lines)
        {
            free(lines[0]);
            memmove(lines, lines + 1, (max_lines - 1) * sizeof(char *));
            line_count = max_lines - 1;
        }

        lines[line_count] = strdup(line_buf);
        line_count++;
    }
    fclose(f);

    *out_count = line_count;
    return lines;
}

void free_install_log_lines(char **lines, int count)
{
    if (!lines)
    {
        return;
    }

    for (int i = 0; i < count; i++)
    {
        free(lines[i]);
    }
    free(lines);
}
