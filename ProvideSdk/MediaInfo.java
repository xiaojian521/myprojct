package com.android.xj.provide;

import android.os.Parcel;
import android.os.Parcelable;
import android.os.Parcelable.Creator;
import com.google.gson.annotations.SerializedName;

public class MediaInfo implements Parcelable {
    private int itemIndex;
    private String itemUUID;
    @SerializedName("itemTitle")
    private String mediaName;
    @SerializedName("itemAuthor")
    private String mediaAuthor;
    @SerializedName("itemImageUrl")
    private String mediaImage;
    @SerializedName("itemType")
    private String mediaType;
    private String mediaGroupName;
    private boolean isFavorable;
    private boolean isFavored;
    private boolean shouldSetPlaymode;
    private int mediaPlaymode;
    private boolean isChangedByPlay;
    public static final Creator<MediaInfo> CREATOR = new Creator<MediaInfo>() {
        public MediaInfo createFromParcel(Parcel in) {
            return new MediaInfo(in);
        }

        public MediaInfo[] newArray(int size) {
            return new MediaInfo[size];
        }
    };

    public MediaInfo() {
    }

    protected MediaInfo(Parcel in) {
        this.itemIndex = in.readInt();
        this.itemUUID = in.readString();
        this.mediaName = in.readString();
        this.mediaAuthor = in.readString();
        this.mediaImage = in.readString();
        this.mediaType = in.readString();
        this.mediaGroupName = in.readString();
        this.isFavorable = in.readByte() != 0;
        this.isFavored = in.readByte() != 0;
        this.shouldSetPlaymode = in.readByte() != 0;
        this.mediaPlaymode = in.readInt();
    }

    public String getMediaGroupName() {
        return this.mediaGroupName;
    }

    public void setMediaGroupName(String mediaGroupName) {
        this.mediaGroupName = mediaGroupName;
    }

    public String getMediaName() {
        return this.mediaName;
    }

    public void setMediaName(String mediaName) {
        this.mediaName = mediaName;
    }

    public String getItemUUID() {
        return this.itemUUID;
    }

    public void setItemUUID(String itemUUID) {
        this.itemUUID = itemUUID;
    }

    public void setItemIndex(int index) {
        this.itemIndex = index;
    }

    public int getItemIndex() {
        return this.itemIndex;
    }

    public String getMediaAuthor() {
        return this.mediaAuthor;
    }

    public void setMediaAuthor(String mediaAuthor) {
        this.mediaAuthor = mediaAuthor;
    }

    public String getMediaImage() {
        return this.mediaImage;
    }

    public void setMediaImage(String mediaImage) {
        this.mediaImage = mediaImage;
    }

    public String getMediaType() {
        return this.mediaType;
    }

    public void setMediaType(String mediaType) {
        this.mediaType = mediaType;
    }

    public boolean isFavorable() {
        return this.isFavorable;
    }

    public void setFavorable(boolean favorable) {
        this.isFavorable = favorable;
    }

    public boolean isFavored() {
        return this.isFavored;
    }

    public void setFavored(boolean favored) {
        this.isFavored = favored;
    }

    public boolean isShouldSetPlaymode() {
        return this.shouldSetPlaymode;
    }

    public void setShouldSetPlaymode(boolean shouldSetPlaymode) {
        this.shouldSetPlaymode = shouldSetPlaymode;
    }

    public int getMediaPlaymode() {
        return this.mediaPlaymode;
    }

    public void setMediaPlaymode(int mediaPlaymode) {
        this.mediaPlaymode = mediaPlaymode;
    }

    public boolean isChangedByPlay() {
        return this.isChangedByPlay;
    }

    public void setChangedByPlay(boolean changedByPlay) {
        this.isChangedByPlay = changedByPlay;
    }

    public String toString() {
        return "mediaIndex: " + this.itemIndex + ", mediaUUID: " + this.itemUUID + ", mediaName: " + this.mediaName + ", mediaAuthor: " + this.mediaAuthor + ", isFavorable: " + this.isFavorable + ", isFavored: " + this.isFavored + ", shouldSetPlaymode: " + this.shouldSetPlaymode + ", mediaPlaymode: " + this.mediaPlaymode;
    }

    public int describeContents() {
        return 0;
    }

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(this.itemIndex);
        dest.writeString(this.itemUUID);
        dest.writeString(this.mediaName);
        dest.writeString(this.mediaAuthor);
        dest.writeString(this.mediaImage);
        dest.writeString(this.mediaType);
        dest.writeString(this.mediaGroupName);
        dest.writeByte((byte)(this.isFavorable ? 1 : 0));
        dest.writeByte((byte)(this.isFavored ? 1 : 0));
        dest.writeByte((byte)(this.shouldSetPlaymode ? 1 : 0));
        dest.writeInt(this.mediaPlaymode);
    }
}
