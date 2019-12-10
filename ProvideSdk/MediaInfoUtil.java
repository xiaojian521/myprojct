package com.android.xj.provide;

import android.os.Bundle;
import android.text.TextUtils;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import java.util.Collections;
import java.util.List;

public class MediaInfoUtil {
    private MediaInfoUtil() {
    }

    public static MediaInfo parseMediaInfo(Bundle bundle) {
        MediaInfo media = new MediaInfo();
        parseBundle(media, bundle);
        return media;
    }

    public static List<MediaInfo> parseMediaInfoList(String json) {
        if (TextUtils.isEmpty(json)) {
            return Collections.emptyList();
        } else {
            try {
                Gson gson = new Gson();
                List<MediaInfo> list = (List)gson.fromJson(json, (new TypeToken<List<MediaInfo>>() {
                }).getType());
                return list;
            } catch (Exception var3) {
                LogUtils.d("MediaInfoUtil", "parseMediaInfoList " + var3);
                return Collections.emptyList();
            }
        }
    }

    private static void parseBundle(MediaInfo media, Bundle bundle) {
        int index = bundle.getInt("media_index");
        media.setItemIndex(index);
        String uuid = bundle.getString("media_uuid");
        media.setItemUUID(uuid);
        String name = bundle.getString("media_name");
        media.setMediaName(name);
        String author = bundle.getString("media_author");
        media.setMediaAuthor(author);
        String type = bundle.getString("media_type");
        media.setMediaType(type);
        String groupName = bundle.getString("media_group_name");
        media.setMediaGroupName(groupName);
        media.setMediaImage(bundle.getString("media_image"));
        media.setFavorable(bundle.getBoolean("media_is_favorable"));
        media.setFavored(bundle.getBoolean("media_is_favored"));
        media.setShouldSetPlaymode(bundle.getBoolean("media_is_set_playmode"));
        media.setMediaPlaymode(bundle.getInt("media_playmode"));
        media.setChangedByPlay(bundle.getBoolean("change_by_play"));
    }
}
